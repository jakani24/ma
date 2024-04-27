@echo off
echo "Downloading Cyberhex installer"
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/ma_installer.exe
echo "Downloading dll files"
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/libcrypto-3-x64.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/libcurl.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/zlib1.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/client_frontend.exe
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/client_backend.exe
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/ma_uninstaller.exe
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/libcurl-d.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/msvcp140.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/vcruntime140.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/vcruntime140_1d.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/msvcp140d.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/vcruntime140d.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/ucrtbased.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/zlibd1.dll
curl -O -L https://github.com/jakani24/cyberhex_bin_distro/raw/main/cyberhex_logo2.ico
echo "Download finished, starting installer"
start ma_installer.exe
