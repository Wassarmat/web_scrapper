include("/home/abyraeg/web_scrapper/build/Desktop_Qt_6_11_1-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/web_scrapper-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/abyraeg/web_scrapper/build/Desktop_Qt_6_11_1-Debug/web_scrapper"
    GENERATE_QT_CONF
)
