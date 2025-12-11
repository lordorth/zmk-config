# Copyright (c) 2021 The ZMK Contributors
# SPDX-License-Identifier: MIT

set(OPENOCD_NRF5_SUBFAMILY nrf52)
board_runner_args(nrfjprog "--nrf-family=NRF52" "--softreset")
include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
include(${ZEPHYR_BASE}/boards/common/openocd-nrf5.board.cmake)

zephyr_library()
zephyr_library_sources(${CMAKE_CURRENT_LIST_DIR}/rgb_power_underglow.c)
zephyr_library_include_directories(${ZEPHYR_BASE}/../zmk/app/include)
