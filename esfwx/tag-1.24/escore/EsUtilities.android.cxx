#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND

JNIEnv* EsUtilities::jniEnvGet()
{
  return TJNIResolver::GetJNIEnv();
}

#else

//JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
//{
//  jniResolverGet().reset(new j2cpp::raw_environment(vm));
//
//  if( !j2cpp::environment::init( jniResolverGet() ) )
//      return JNI_ERR;
//
//  return JNI_VERSION_1_4;
//}
////---------------------------------------------------------------------------
//
//EsUtilities::EsJniResolver& EsUtilities::jniResolverGet()
//{
//  static EsUtilities::EsJniResolver s_inst;
//
//  return s_inst;
//}
////---------------------------------------------------------------------------

#endif

JavaVM* EsUtilities::jvmGet()
{
  JNIEnv* env = jniEnvGet();
  ES_ASSERT(env);

  JavaVM* jvm = 0;
  env->GetJavaVM(&jvm);

  return jvm;
}
//---------------------------------------------------------------------------

void EsUtilities::urlOpen(const EsString& url)
{
  _di_JIntent intent = TJIntent::JavaClass->init(
    TJIntent::JavaClass->ACTION_VIEW,
    TJnet_Uri::JavaClass->parse(
      StringToJString(
        EsString::toString(
          url,
          EsString::StrNoCEscape
        ).c_str()
      )
    )
  );

  SharedActivity->startActivity(
    intent
  );
}
//---------------------------------------------------------------------------

