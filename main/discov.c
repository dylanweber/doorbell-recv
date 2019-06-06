/*
	MQTT Reciever - discov.c
	Copyright 2019 Dylan Weber

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
 */
#include "discov.h"

static const char *TAG = "discov";

esp_err_t init_mdns() {
	if (xSemaphoreTake(connected_semaphore, portMAX_DELAY)) {
		esp_err_t ret = mdns_init();
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "Failure initializing mDNS");
			return ESP_FAIL;
		}

		mdns_hostname_set(CONFIG_MDNS_NAME);
		mdns_instance_name_set(CONFIG_MDNS_INSTANCE);

		mdns_result_t *results;
		ret = mdns_query_ptr("_mqtt", "_tcp", 10000, 1, &results);
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "Failure finding mqtt broker via mDNS");
			return ESP_FAIL;
		}

		if (results != NULL) {
			ip_addr_t conn_addr = results->addr->addr;

			if (conn_addr.type == IPADDR_TYPE_V6) {
				ESP_LOGI(TAG, "Service IPv6: " IPV6STR, IPV62STR(conn_addr.u_addr.ip6));
			} else {
				ESP_LOGI(TAG, "Service IPv4: " IPSTR, IP2STR(&(conn_addr.u_addr.ip4)));
			}

			mdns_query_results_free(results);
			return ESP_OK;
		} else {
			ESP_LOGI(TAG, "Could not find MQTT broker.");
		}
	}
	return ESP_FAIL;
}