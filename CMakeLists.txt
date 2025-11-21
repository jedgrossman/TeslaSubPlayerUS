cmake_minimum_required(VERSION 3.10)

project(tesla_subghz_launcher)

flipper_create_app(
    TARGET tesla_subghz_launcher
    SOURCES app.c
    SDK furi
)
