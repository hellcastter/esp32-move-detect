idf_component_register(SRCS
        "main.cpp"
        "Server.cpp"
        "Camera.cpp"

        "ProcessorDefault.cpp"
        "ProcessorMedianDif.cpp"
        "ProcessorMedianBox.cpp"
        "ProcessorDifferenceBox.cpp"
        "ProcessorDifference.cpp"
        "ProcessorAverageDif.cpp"
        "ProcessorAverageBox.cpp"

        "WifiController.cpp"

        INCLUDE_DIRS "."
)
