#include <jni.h>
#include <iostream>

int main(int argc, char* argv[]) {
    //if (argc != 2) {
    //    std::cerr << "Usage: " << argv[0] << " <path_to_image_file>" << std::endl;
    //    return 1;
    //}

    //const char* filePath = argv[1];

    // Path to the Java wrapper class
    const char* classpathOption =
        "-Djava.class.path=D:/Projects/Bioformats-C-Wrapper/build/java/BioFormatsWrapper.jar;"
        "D:/Projects/Bioformats-C-Wrapper/build/java/bioformats_package.jar;"
        "D:/Projects/Bioformats-C-Wrapper/build/java/bio-formats-tools.jar;"
        "D:/Projects/Bioformats-C-Wrapper/build/java/formats-gpl.jar;"
        "D:/Projects/Bioformats-C-Wrapper/build/java/formats-api.jar;";

     // JNI initialization
     JavaVMOption options[1];
     options[0].optionString = const_cast<char*>(classpathOption);

     JavaVMInitArgs vmArgs;
     vmArgs.version = JNI_VERSION_1_8; // or your installed JDK version
     vmArgs.nOptions = 1;
     vmArgs.options = options;
     vmArgs.ignoreUnrecognized = false;

     JavaVM *jvm;
     JNIEnv *env;
     jint result = JNI_CreateJavaVM(&jvm, reinterpret_cast<void**>(&env), &vmArgs);
     if (result != JNI_OK) {
         std::cerr << "Error creating JVM: " << result << std::endl;
         return 1;
     }
     else {
         std::cout << "JVM succesfully created." << std::endl;
     }

      jclass wrapperClass = env->FindClass("BioFormatsWrapper");
      if (wrapperClass == nullptr) {
          std::cerr << "Error: BioFormatsWrapper Class not found." << std::endl;
          jvm->DestroyJavaVM();
          return -1;
      }

      jmethodID constructor = env->GetMethodID(wrapperClass, "<init>", "()V");
      jobject wrapperInstance = env->NewObject(wrapperClass, constructor);

      jmethodID getMetadataMethod = env->GetMethodID(wrapperClass, "getMetadata", "(Ljava/lang/String;)Ljava/lang/String;");
      jstring filePath = env->NewStringUTF("/path/to/your/image/file");
      jstring metadata = (jstring)env->CallObjectMethod(wrapperInstance, getMetadataMethod, filePath);

      if (metadata != nullptr) {
          const char* metadataChars = env->GetStringUTFChars(metadata, nullptr);
          std::cout << metadataChars << std::endl;
          env->ReleaseStringUTFChars(metadata, metadataChars);
      }
      else {
          std::cerr << "Error retrieving metadata" << std::endl;
      }

      // Destroy the JVM and clean up
      jvm->DestroyJavaVM();
      return 0;
}