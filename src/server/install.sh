#!/bin/bash
docker volume create cyberhex-db-storage
mkdir -p /var/www/html/database_srv
mkdir -p /var/www/html/export
mkdir -p /var/www/html/import
chown -R www-data:www-data /var/www/html/export/
chown -R www-data:www-data /var/www/html/import/
chown -R www-data:www-data /var/www/html/install/
chown -R www-data:www-data /var/www/html/database_srv/