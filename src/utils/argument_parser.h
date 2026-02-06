#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <functional>
#include <map>
#include <string>
#include <vector>

enum class OptionType
{
    FLAG,     // 标志选项，不需要参数（如--verbose）
    REQUIRED, // 必须带参数的选项（如--output=file）
    OPTIONAL  // 可选参数的选项
};

// 选项结构体
struct Option
{
    char short_opt;            // 短选项（如 'h' 对应 -h）
    std::string long_opt;      // 长选项（如 "help" 对应 --help）
    OptionType type;           // 选项类型
    std::string description;   // 选项描述
    std::string default_value; // 默认值（可选）
};

class ArgumentParser
{
private:
    std::string program_name;
    std::string description;
    std::vector<Option> options;
    std::map<std::string, std::string> parsed_values;
    std::vector<std::string> positional_args;

    // 检查选项是否存在
    bool
    has_option(const std::string &long_opt) const;
    bool
    has_option(char short_opt) const;

    // 查找选项
    const Option *
    find_option(const std::string &long_opt) const;
    const Option *
    find_option(char short_opt) const;

public:
    // 构造函数
    ArgumentParser(const std::string &name, const std::string &desc);

    // 添加选项
    void
    add_option(const Option &opt);

    // 解析命令行参数
    bool
    parse(int argc, char *argv[]);

    // 检查选项是否被设置
    bool
    has(const std::string &long_opt) const;
    bool
    has(char short_opt) const;

    // 获取选项值
    std::string
    get(const std::string &long_opt) const;
    std::string
    get(char short_opt) const;

    // 获取位置参数
    const std::vector<std::string> &
    get_positional_args() const;

    // 打印帮助信息
    void
    print_help() const;
};

#endif // ARGUMENT_PARSER_H
