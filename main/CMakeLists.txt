set(COMPONENT_SRCS 
    "app_main.c"
    "app_settings.c" 
    "app_wifi.c" 
    "app_bt.c" 
    "app_httpd.c" 
    "app_sntp.c" 

)

set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES
    mdns
    nvs_flash
    esp_http_server
    bt
)

set(COMPONENT_EMBED_FILES
    "www/index.html.gz"
    "www/style.css.gz"
    "www/script.js.gz"
    "www/favicon.ico.gz"
)

register_component()
