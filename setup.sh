sudo DEBIAN_FRONTEND=noninteractive apt-get update -y
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y python3 python3-pip python3-venv build-essential make gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu

echo "Anti pattern go brrrr"
curl -sSL https://install.python-poetry.org | python3 -
