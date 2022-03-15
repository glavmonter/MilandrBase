function(set_options TARGET LSCRIPT FLASH_START FLASH_SIZE)
    target_link_options(${TARGET}.elf PRIVATE -T ${LSCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${TARGET}.map)
    add_custom_command(TARGET ${TARGET}.elf POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${TARGET}.elf> ${PROJECT_BINARY_DIR}/${TARGET}.hex
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET}.elf> ${PROJECT_BINARY_DIR}/${TARGET}.bin
            COMMENT "Building ${TARGET}.hex Building ${TARGET}.bin")

    add_custom_command(TARGET ${TARGET}.elf POST_BUILD
            COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET}.elf> > ${PROJECT_BINARY_DIR}/${TARGET}.S
            COMMENT "Dump listing for ${TARGET}.elf")

# TODO Раскомментировать для дополнения выходного hex единицами и контрольной суммой
#    add_custom_command(TARGET ${TARGET}.elf POST_BUILD
#            COMMAND ${PYTHON_VENV_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Tools/append_crc.py ${FLASH_START_ADDRESS} ${FLASH_SIZE} ${CMAKE_BINARY_DIR}/${TARGET}.bin ${CMAKE_BINARY_DIR}/${TARGET}.hex > ${TARGET}.crc.txt
#            COMMENT "Append CRC for ${TARGET}.bin and ${TARGET}.hex" )

    set_property(
            TARGET ${TARGET}.elf
            APPEND
            PROPERTY ADDITIONAL_CLEAN_FILES ${TARGET}.map ${TARGET}.hex ${TARGET}.bin ${TARGET}.S ${TARGET}.crc.txt
    )
endfunction()
