# Copyright(C) 2024 eSOL Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

set(RMW_ZENOH_PICO_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_attach.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_condition.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_data.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_entity.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_gid.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_liveliness.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_messageType.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_nodeInfo.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_rosMessage.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_session.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_string.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/zenoh_pico/zenoh_pico_topicInfo.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/utilities/test_qos_profile.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_client.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_count.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_dynamic_message.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_event.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_features.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_gid.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_guard_condition.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_identifier.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_init.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_logging.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_names.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_node.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_publisher.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_qos.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_serialize.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_service.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_subscription.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_take.c
    ${CMAKE_CURRENT_LIST_DIR}/../src/rmw_wait.c)
