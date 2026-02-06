# 源代码名称：SmartParkingDetectSDK-HP-VZ
简述：用于臻识相机高位停车算法项目的代码管理，存储算法核心代码、模型文件、测试脚本及相关技术文档。

## 目录结构：
config：配置文件目录  
docs：用于保存开发文档和书面协议定义  
models：模型文件目录  
src：源代码目录  
&ensp;|-- workflow：工作流目录    
&ensp;|-- models_processor:模型前后处理目录，包含模型加载、推理、后处理等功能  
&ensp;|-- managers：管理目录，包含全局变量、资源管理、状态维护等功能，handle资源导出类  
&ensp;|-- utils：工具目录，包含通用函数、类、结构体等  
&ensp;|-- interfaces：接口目录，包含与外部系统交互的代码，如API    
&ensp;|-- vehinout：泊位识别目录
test：测试目录  
third_party：第三方库目录  
scripts：脚本目录，包含自动化脚本、构建脚本、部署脚本等  
