QT       += core gui sql widgets printsupport charts

TARGET = EmployeeManagementSystem
TEMPLATE = app

# Use C++17 standard
CONFIG += c++17

# Recursively include all source (.cpp) and header (.h) files
SOURCES += $$files(*.cpp, true)
HEADERS += $$files(*.h, true)

# Exclude Qt generated files to prevent build conflicts
# (moc_*.cpp are generated from headers, qrc_*.cpp from resources)
SOURCES -= $$files(moc_*.cpp, true)
SOURCES -= $$files(qrc_*.cpp, true)

# Handle Resources
# If resources.qrc exists, use it to generate resources
exists(resources.qrc) {
    RESOURCES += resources.qrc
} else:exists(qrc_resources.cpp) {
    # Fallback: If resources.qrc is missing but qrc_resources.cpp exists, compile it directly
    SOURCES += qrc_resources.cpp
}

# Include UI files if they exist
FORMS += $$files(*.ui, true)