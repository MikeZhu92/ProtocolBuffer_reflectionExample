#include <iostream>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>

#include "user_def.pb.h"

using namespace google::protobuf;
//using namespace google::protobuf::compiler;

using namespace std;


class MockErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector {
 public:
  MockErrorCollector() {}
  ~MockErrorCollector() {}

  std::string text_;

  // implements ErrorCollector ---------------------------------------
  void AddError(const std::string& filename, int line, int column,
                const std::string& message) {
      std::cerr << "error filename "<<filename <<" message "<<message <<std::endl;
  }
};


MockErrorCollector errCollector;
google::protobuf::compiler::DiskSourceTree sourceTree; 
google::protobuf::compiler::Importer importer(&sourceTree, &errCollector);

google::protobuf::Message *createMessage(const std::string &type_name){
    google::protobuf::Message *message = NULL;
    const Descriptor *descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
    if (descriptor){
        const Message *prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype){
          message = prototype->New();
        }
    }else {
        descriptor = importer.pool()->FindMessageTypeByName(type_name);
    DynamicMessageFactory *dynamicMessageFactory = new DynamicMessageFactory();
        if(descriptor){
              message = dynamicMessageFactory->GetPrototype(descriptor)->New();
        }else{
              std::cerr << "not found message type " << type_name << std::endl;
        }
    }
    return message;
}

int main() {
 
    sourceTree.MapPath("", ".");

    importer.Import("user_def.proto");

    // 使用上面的函数创建一个新的UserInfo message
    google::protobuf::Message *msg = createMessage("user.UserInfo");
    if (NULL == msg) {
        // 创建失败，可能是消息名错误，也可能是编译后message解析器
        // 没有链接到主程序中。
        cout<<"link error"<<endl;
        return -1;
    }

    // 获取message的descriptor
    const Descriptor* descriptor = msg->GetDescriptor();
    // 获取message的反射接口，可用于获取和修改字段的值
    const Reflection* reflection = msg->GetReflection();

    // 根据字段名查找message的字段descriptor
    const FieldDescriptor* idField = descriptor->FindFieldByName("id");
    // 将id设置为100
    if (NULL != idField) {
        reflection->SetInt64(msg, idField, 100);
        cout<<reflection->GetInt64(*msg,idField)<<endl;
    }

    // ... 其他操作

    // 最后删除message
    delete msg;

    return 0;
}