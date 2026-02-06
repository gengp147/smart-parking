#include "argument_parser.h"
#include <iostream>
#include <algorithm>
#include <sstream>

ArgumentParser::ArgumentParser(const std::string& name, const std::string& desc)
    : program_name(name), description(desc) {}

void ArgumentParser::add_option(const Option& opt) {
    // 检查选项是否已存在
    if (has_option(opt.long_opt) || has_option(opt.short_opt)) {
        throw std::invalid_argument("Option already exists");
    }
    
    options.push_back(opt);
    
    // 如果有默认值，先设置默认值
    if (!opt.default_value.empty()) {
        parsed_values[opt.long_opt] = opt.default_value;
    }
}

bool ArgumentParser::has_option(const std::string& long_opt) const {
    return find_option(long_opt) != nullptr;
}

bool ArgumentParser::has_option(char short_opt) const {
    return find_option(short_opt) != nullptr;
}

const Option* ArgumentParser::find_option(const std::string& long_opt) const {
    for (const auto& opt : options) {
        if (opt.long_opt == long_opt) {
            return &opt;
        }
    }
    return nullptr;
}

const Option* ArgumentParser::find_option(char short_opt) const {
    for (const auto& opt : options) {
        if (opt.short_opt == short_opt) {
            return &opt;
        }
    }
    return nullptr;
}

bool ArgumentParser::parse(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // 处理长选项（--option 或 --option=value）
        if (arg.substr(0, 2) == "--") {
            std::string opt_name;
            std::string opt_value;
            
            size_t eq_pos = arg.find('=');
            if (eq_pos != std::string::npos) {
                opt_name = arg.substr(2, eq_pos - 2);
                opt_value = arg.substr(eq_pos + 1);
            } else {
                opt_name = arg.substr(2);
            }
            
            const Option* opt = find_option(opt_name);
            if (!opt) {
                std::cerr << "错误: 未知选项 --" << opt_name << std::endl;
                return false;
            }
            
            // 处理不同类型的选项
            if (opt->type == OptionType::FLAG) {
                if (eq_pos != std::string::npos) {
                    std::cerr << "错误: 选项 --" << opt_name << " 不接受参数" << std::endl;
                    return false;
                }
                parsed_values[opt_name] = "true";
            } else if (opt->type == OptionType::REQUIRED) {
                if (eq_pos == std::string::npos) {
                    // 参数在 next argument
                    if (i + 1 >= argc) {
                        std::cerr << "错误: 选项 --" << opt_name << " 需要参数" << std::endl;
                        return false;
                    }
                    opt_value = argv[++i];
                }
                parsed_values[opt_name] = opt_value;
            }
        }
        // 处理短选项（-o 或 -ovalue 或 -o value）
        else if (arg[0] == '-' && arg.length() > 1) {
            for (size_t j = 1; j < arg.length(); ++j) {
                char opt_char = arg[j];
                const Option* opt = find_option(opt_char);
                
                if (!opt) {
                    std::cerr << "错误: 未知选项 -" << opt_char << std::endl;
                    return false;
                }
                
                // 处理不同类型的选项
                if (opt->type == OptionType::FLAG) {
                    parsed_values[opt->long_opt] = "true";
                } else if (opt->type == OptionType::REQUIRED) {
                    std::string opt_value;
                    
                    // 检查当前参数中是否有剩余字符作为值
                    if (j + 1 < arg.length()) {
                        opt_value = arg.substr(j + 1);
                        j = arg.length(); // 跳到当前参数末尾
                    } else {
                        // 参数在 next argument
                        if (i + 1 >= argc) {
                            std::cerr << "错误: 选项 -" << opt_char << " 需要参数" << std::endl;
                            return false;
                        }
                        opt_value = argv[++i];
                        j = arg.length(); // 跳到当前参数末尾
                    }
                    
                    parsed_values[opt->long_opt] = opt_value;
                }
            }
        }
        // 位置参数
        else {
            positional_args.push_back(arg);
        }
    }
    
    // 检查所有必填选项是否都提供了
    for (const auto& opt : options) {
        if (opt.type == OptionType::REQUIRED && 
            !has(opt.long_opt) && 
            opt.default_value.empty()) {
            std::cerr << "错误: 缺少必需选项 --" << opt.long_opt << " (-" << opt.short_opt << ")" << std::endl;
            return false;
        }
    }
    
    return true;
}

bool ArgumentParser::has(const std::string& long_opt) const {
    return parsed_values.find(long_opt) != parsed_values.end();
}

bool ArgumentParser::has(char short_opt) const {
    const Option* opt = find_option(short_opt);
    if (!opt) return false;
    return has(opt->long_opt);
}

std::string ArgumentParser::get(const std::string& long_opt) const {
    auto it = parsed_values.find(long_opt);
    if (it != parsed_values.end()) {
        return it->second;
    }
    return "";
}

std::string ArgumentParser::get(char short_opt) const {
    const Option* opt = find_option(short_opt);
    if (!opt) return "";
    return get(opt->long_opt);
}

const std::vector<std::string>& ArgumentParser::get_positional_args() const {
    return positional_args;
}

void ArgumentParser::print_help() const {
    std::cout << "用法: " << program_name << " [选项] [位置参数...]" << std::endl;
    if (!description.empty()) {
        std::cout << std::endl << description << std::endl;
    }
    std::cout << std::endl << "选项:" << std::endl;
    
    // 计算最长选项名长度，用于对齐
    size_t max_opt_length = 0;
    for (const auto& opt : options) {
        std::stringstream ss;
        ss << "  -" << opt.short_opt << ", --" << opt.long_opt;
        if (opt.type == OptionType::REQUIRED) {
            ss << " <参数>";
        }
        max_opt_length = std::max(max_opt_length, ss.str().length());
    }
    
    // 打印选项
    for (const auto& opt : options) {
        std::stringstream ss;
        ss << "  -" << opt.short_opt << ", --" << opt.long_opt;
        if (opt.type == OptionType::REQUIRED) {
            ss << " <参数>";
        }
        
        std::string opt_str = ss.str();
        std::cout << opt_str;
        
        // 对齐描述
        if (opt_str.length() < max_opt_length) {
            std::cout << std::string(max_opt_length - opt_str.length(), ' ');
        } else {
            std::cout << "  ";
        }
        
        std::cout << opt.description;
        
        // 显示默认值
        if (!opt.default_value.empty()) {
            std::cout << " (默认: " << opt.default_value << ")";
        }
        
        std::cout << std::endl;
    }
}
