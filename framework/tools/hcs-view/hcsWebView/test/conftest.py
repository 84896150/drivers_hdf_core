#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd. 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#
# http://www.apache.org/licenses/LICENSE-2.0 
#
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License. 

import pytest
from selenium import webdriver
from selenium.webdriver.chrome.webdriver import WebDriver

global_param = {}


@pytest.fixture
def _set_global_data(global_driver):
    driver = None
    global_param[global_driver] = driver


@pytest.fixture
def _get_global_data(global_driver):
    return global_param.get(global_driver)


def pytest_addoption(parser):
    parser.addoption(
        "--browser", action="store", default="chrome", help="browser option: firefox or chrome"
             )


@pytest.fixture(scope='session')
def browser(request):
    driver = _get_global_data('global_driver')
    if driver is None:
        name = request.config.getoption("--browser")
        if name == "firefox":
            driver = webdriver.Firefox()
        elif name == "chrome":
            driver = webdriver.Chrome()
        else:
            driver = webdriver.Firefox()

    def fn():
        driver.quit()

    request.addfinalizer(fn)
    return driver
