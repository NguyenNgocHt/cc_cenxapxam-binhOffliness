#define GAMEINFO	"{\"version\":1,\"appName\":\"CenVipLa\",\"runMode\":0}"

#include "cocos2d.h"
#include "AppDelegate.h"
#include "cocos/scripting/js-bindings/manual/jsb_module_register.hpp"
#include "cocos/scripting/js-bindings/manual/jsb_global.h"
#include "cocos/scripting/js-bindings/jswrapper/SeApi.h"
#include "cocos/scripting/js-bindings/event/EventDispatcher.h"
#include "cocos/scripting/js-bindings/manual/jsb_classtype.hpp"

USING_NS_CC;

#include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"
#include "cocos/network/HttpClient.h"
using namespace cocos2d::network;

static bool install_cert(se::State& s)
{
    const auto& args = s.args();
    int argc = (int)args.size();
    if (argc >= 1)
    {
        bool ok = false;
        std::string file;
        ok = seval_to_std_string(args[0], &file);
        SE_PRECONDITION2(ok, false, "args[0] isn't a string.");
        HttpClient::getInstance()->setSSLVerification(file);
        return true;
    }

    return false;
}
SE_BIND_FUNC(install_cert)

bool jsb_register_install_cert(se::Object* global)
{
    global->defineFunction("installCert", _SE(install_cert));
    return true;
}



static bool get_Data_info(se::State& s)
{
    s.rval().setString(GAMEINFO);
    return true;
}

SE_BIND_FUNC(get_Data_info)

bool jsb_register_get_game_info(se::Object* global)
{
    global->defineFunction("getAppInfo", _SE(get_Data_info));
    return true;
}

AppDelegate::AppDelegate(int width, int height) : Application("game", width, height)
{
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    se::ScriptEngine *se = se::ScriptEngine::getInstance();

    jsb_set_xxtea_key("c456dcfd-9c3f-4b");
    jsb_init_file_operation_delegate();


    se->setExceptionCallback([](const char *location, const char *message, const char *stack) {
    });

    jsb_register_all_modules();

    se->addRegisterCallback(jsb_register_install_cert);

    se->addRegisterCallback(jsb_register_get_game_info);

    se->start();

    se::AutoHandleScope hs;
    jsb_run_script("jsb-adapter/jsb-builtin.js");
    jsb_run_script("main.js");

    se->addAfterCleanupHook([]() {
        JSBClassType::destroy();
    });

    return true;
}

void AppDelegate::onPause()
{
    EventDispatcher::dispatchOnPauseEvent();
}

void AppDelegate::onResume()
{
    EventDispatcher::dispatchOnResumeEvent();
}
