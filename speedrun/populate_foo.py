import requests

url = "http://localhost:5000"

path_list = "/demo/foo/list"
path_create = "/demo/foo/create"

payload = {"foo": "i am foo", "bar": "i am bar"}


r = requests.post(f"{url}{path_create}", json=payload)
print(r.status_code, r.json())

r = requests.get(f"{url}{path_list}")
print(r.json())
