set_property (DIRECTORY APPEND PROPERTY LABELS Assets)

# Reference the Demo resources
set(DEMO_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Demo/Resources")

juce_add_binary_data (UnitTestAssets SOURCES
						${DEMO_RESOURCES_DIR}/1.webp
						${DEMO_RESOURCES_DIR}/2.webp
						${DEMO_RESOURCES_DIR}/ballon_8bit.bmp
						${DEMO_RESOURCES_DIR}/ballon.bmp
						${DEMO_RESOURCES_DIR}/Castle.jpg
						${DEMO_RESOURCES_DIR}/icon.png
						${DEMO_RESOURCES_DIR}/IMG_1883.JPG
						${DEMO_RESOURCES_DIR}/Landscape_1.jpg
						${DEMO_RESOURCES_DIR}/Leaf.jpg
						${DEMO_RESOURCES_DIR}/mountain.jpg
						${DEMO_RESOURCES_DIR}/pencils.jpeg
						${DEMO_RESOURCES_DIR}/SVG_example_markup_grid.svg
						${DEMO_RESOURCES_DIR}/Analog_PWM_Saw_01.wav
					)
