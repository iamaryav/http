import requests

def get_request():
    url = "http://localhost:8080/hello.json"
    response = requests.get(url)
    print(response.json())


get_request()



