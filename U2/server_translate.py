import requests
from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import os
from dotenv import load_dotenv

load_dotenv()
X_RAPIDAPI_KEY = os.getenv("X_RAPIDAPI_KEY")

def translate_text(text, target_language):
    url = "https://google-translate1.p.rapidapi.com/language/translate/v2"

    payload = {"q": text, "target": target_language, "source": "en"}
    headers = {
        "content-type": "application/x-www-form-urlencoded",
        "Accept-Encoding": "application/gzip",
        "X-RapidAPI-Key": X_RAPIDAPI_KEY,
        "X-RapidAPI-Host": "google-translate1.p.rapidapi.com",
    }

    response = requests.post(url, data=payload, headers=headers)
    translated_text = response.json()["data"]["translations"][0]["translatedText"]
    return translated_text

class MyHTTPRequestHandler(BaseHTTPRequestHandler):
    def _set_response(self, content_type="text/plain"):
        self.send_response(200)
        self.send_header("Content-type", content_type)
        self.end_headers()

    def throw_custom_error(self, message):
        self._set_response("application/json")
        self.wfile.write(json.dumps({"message": message}).encode())

    def do_GET(self):
        self._set_response()
        self.wfile.write("Hello from the server!".encode())

    def do_POST(self):
        content_length = int(self.headers["Content-Length"])
        post_data = self.rfile.read(content_length).decode()

        try:
            data = json.loads(post_data)
        except:
            self.throw_custom_error("Invalid JSON")
            return


        # Parse JSON data from the POST request
        """data = json.loads(post_data)"""

        if "text" in data and "target_language" in data:
            text = data["text"]
            target_language = data["target_language"]
            translated_text = translate_text(text, target_language)
            response_data = {"message": "Traduccion existosa", "Texto traduccido": translated_text}
        else:
            response_data = {"message": "Invalid request"}

        # Respond to the client with JSON data
        self._set_response("application/json")
        self.wfile.write(json.dumps(response_data).encode())
        
def run_server(server_class=HTTPServer, handler_class=MyHTTPRequestHandler, port=7800):
    server_address = ("", port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting server on port {port}...")
    httpd.serve_forever()


if __name__ == "__main__":
    run_server()