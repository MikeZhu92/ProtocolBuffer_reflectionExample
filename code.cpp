//magic number
#define II 1
#define SS "case"
#define DD 1.1

using namespace std;


/*
	Input: the name of your pb class(should exactly be the same with the name in *.proto)
	Output: pointer to the new pb message created by the pb name 
	used to create a message by its name dynamically, especially in runtime after compiled

*/
google::protobuf::Message *createMessage(const std::string &typeName) {
    google::protobuf::Message *message = NULL;
    const google::protobuf::Descriptor *descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);

    if (descriptor) {
        const google::protobuf::Message *prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);

        if (prototype) {
            message = prototype->New();
        }
    }

    return message;
}


/*
	Input: the pointer to the message need to delete
	Output:  
	used to delete a message pointer after you createMessage()

*/
void ReleaseMessage(google::protobuf::Message *pMsg) {
	if (NULL != pMsg) {
		pMsg->Clear();
		delete pMsg;
	}
}



/*
	Input: 1.the pointer to the message need to be set (necessary)
		   2.the Descriptor of that message (necessary)
		   3.the index of the current message (optional, it doesn't deal with setting the value)
		   4.the iRowNum means if there is a repeat type, how much times you want to set(default 1 times)
	Output:
	used to set your value to your pb message (recursively, if there is sub message)
	
*/
void SetM(google::protobuf::Message *pMsg, const google::protobuf::Descriptor *pDescriptor, int index = 0, int iRowNum = 1) {
    //get the reflection
    const google::protobuf::Reflection *pReflection = pMsg->GetReflection();
    assert(NULL != pMsg);
	assert(NULL != pReflection);
	assert(NULL != pDescriptor);
	
	//go through all the field of the pb class
    for (int i = 0; i < pDescriptor->field_count(); ++i) {
        const google::protobuf::FieldDescriptor *pFieldDescriptor = pDescriptor->field(i);
        assert(NULL != pFieldDescriptor);

        const std::string &strFieldName = pFieldDescriptor->name();
        std::cout << strFieldName << " :  ";

        //according to the type, set value to the field
        switch (pFieldDescriptor->type()) {
        case google::protobuf::FieldDescriptor::TYPE_STRING:
            pReflection->SetString(pMsg, pFieldDescriptor, SS);//fix the filed with some magic number!OAO
            std::cout << pReflection->GetString(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT32:
            pReflection->SetInt32(pMsg, pFieldDescriptor, II + index);//fix the filed with some magic number!OAO
            std::cout << pReflection->GetInt32(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            pReflection->SetString(pMsg, pFieldDescriptor, SS);//fix the filed with some magic number!OAO
            std::cout << pReflection->GetString(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            pReflection->SetDouble(pMsg, pFieldDescriptor, DD + index);//fix the filed with some magic number!OAO
            std::cout << pReflection->GetDouble(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
            pReflection->SetInt64(pMsg, pFieldDescriptor, II + index);//fix the filed with some magic number!OAO
            std::cout << pReflection->GetInt64(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
            //for there is sub messege
            if (pFieldDescriptor->label() == pFieldDescriptor->LABEL_REPEATED) {
                for (int i = 0; i < iRowNum; i++) {
                    google::protobuf::Message *pSMsg = pReflection->AddMessage(pMsg, pFieldDescriptor);
                    const google::protobuf::Descriptor *pSDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(pFieldDescriptor->message_type()->name());
                    SetM(pSMsg, pSDescriptor, i);
                }
            } else {
                google::protobuf::Message *pSMsg = pReflection->AddMessage(pMsg, pFieldDescriptor);
                const google::protobuf::Descriptor *pSDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(pFieldDescriptor->message_type()->name());
                SetM(pSMsg, pSDescriptor, 0);
            }
            break;
        }

        default:
            cout << "default reflection" << endl;
            break;
        }
    }
}


/*
	Input: 1.the pointer to the message need to be set (necessary)
		   2.the Descriptor of that message (necessary)
		   3.the index of the current message (optional, it doesn't deal with the value)
	Output:
	used to get your value from your pb message and print(recursively, if there is sub message)
	
*/
void GetM(google::protobuf::Message *pMsg, const google::protobuf::Descriptor *pDescriptor, int index = 0){
	const google::protobuf::Reflection *pReflection = pMsg->GetReflection();
    assert(NULL != pMsg);
	assert(NULL != pReflection);
	assert(NULL != pDescriptor);
	
	//go through all the field of the pb class
	for (int i = 0; i < pDescriptor->field_count(); ++i) {
        const google::protobuf::FieldDescriptor *pFieldDescriptor = pDescriptor->field(i);
        assert(NULL != pFieldDescriptor);

        const std::string &strFieldName = pFieldDescriptor->name();
        std::cout << strFieldName << " :  ";

        //according to the type of current field, print the value
        switch (pFieldDescriptor->type()) {
        case google::protobuf::FieldDescriptor::TYPE_STRING:
            std::cout << pReflection->GetString(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT32:
            std::cout << pReflection->GetInt32(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            std::cout << pReflection->GetString(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            std::cout << pReflection->GetDouble(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
            std::cout << pReflection->GetInt64(*pMsg, pFieldDescriptor) << std::endl;
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
		//if there is sub message	
            if (pFieldDescriptor->label() == pFieldDescriptor->LABEL_REPEATED) {
				//for repeat message, get the repeat times
                int iSize = pReflection->FieldSize(*pMsg, pFieldDescriptor);
				//repeat iSize times print the value
                for (int i = 0; i < iSize; i++) {
                    const google::protobuf::Message *pSMsg = &pReflection->GetRepeatedMessage(*pMsg, pFieldDescriptor, i);
                    const google::protobuf::Descriptor *pSDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(pFieldDescriptor->message_type()->name());
                    GetM(pSMsg, pSDescriptor,i);
                }
            } else {
                const google::protobuf::Message *pSMsg = &pReflection->GetMessage(*pMsg, pFieldDescriptor);
                const google::protobuf::Descriptor *pSDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(pFieldDescriptor->message_type()->name());
                GetM(pSMsg, pSDescriptor,0);
            }
            break;
        }

        default:
            cout << "default reflection" << endl;
            break;
        }
    }
}


//an example 
void testRepeat2(){
	string mstrCurMsg = "LoginRespMessage";//the name of my pb class is LoginRespMessage

	google::protobuf::Message *pMsg = createMessage(mstrCurMsg);//create the message
	cout << "CREATE done" << endl;
	
	//get the descriptor
	const	google::protobuf::Descriptor *pDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(mstrCurMsg);

	SetM(pMsg, pDescriptor, 0, 5);//it means the current index of the message is 0, and for repeat field, it will repeat 5 times
	cout << "SET done" << endl;
	
	GetM(pMsg, pDescriptor);
	cout << "GET done" << endl;
	
	ReleaseMessage(pMsg);
	pDescriptor=NULL;
	delete pDescriptor;
	
}