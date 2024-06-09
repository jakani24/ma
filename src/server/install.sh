#!/bin/bash
docker volume create cyberhex-db-storage
mkdir -p cyberhex-code/database_srv
mkdir -p cyberhex-code/export
mkdir -p cyberhex-code/import
chown -R www-data:www-data cyberhex-code/export/
chown -R www-data:www-data cyberhex-code/import/
chown -R www-data:www-data cyberhex-code/install/
chown -R www-data:www-data cyberhex-code/database_srv/