include("/home/abyraeg/web_scrapper/build/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/web_scrapper-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/abyraeg/web_scrapper/build/web_scrapper"
    GENERATE_QT_CONF
)
