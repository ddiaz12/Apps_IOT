# Importa las clases necesarias del módulo http.server y json
from http.server import BaseHTTPRequestHandler, HTTPServer
import json

# Inicializa un contador en 0
contador = 0

# Define una clase personalizada que hereda de BaseHTTPRequestHandler
class MyHTTPRequestHandler(BaseHTTPRequestHandler):
    def _set_response(self, content_type="text/plain"):
        # Configura la respuesta HTTP con el código 200 (OK) y el tipo de contenido especificado
        self.send_response(200)
        self.send_header("Content-type", content_type)
        self.end_headers()

    def do_GET(self):
        # Maneja las solicitudes GET
        self._set_response()
        respuesta = "el valor es: " + str(contador)
        self.wfile.write(respuesta.encode())

    def do_POST(self):
        # Maneja las solicitudes POST
        content_length = int(self.headers["Content-Length"])
        post_data = self.rfile.read(content_length)
        
        # Decodifica los datos JSON del cuerpo de la solicitud POST
        body_json = json.loads(post_data.decode())
        
        # Comprueba si el cuerpo JSON contiene las claves 'action' y 'quantity'
        global contador
        if 'action' in body_json and 'quantity' in body_json:
            action = body_json['action']
            quantity = int(body_json['quantity'])
            
            # Realiza operaciones en el contador basadas en la acción
            if action == 'asc':
                contador += quantity  
            elif action == 'desc':
                contador -= quantity  
        
        # Imprime la solicitud HTTP completa en la consola
        print("\n----- Incoming POST Request -----")
        print(f"Requestline: {self.requestline}")
        print(f"Headers:\n{self.headers}")
        print(f"Body:\n{post_data.decode()}")
        print("-------------------------------")

        # Responde al cliente con un mensaje JSON
        response_data = json.dumps({"message": "Received POST data", 
                                    "data": post_data.decode()})
        self._set_response("application/json")
        self.wfile.write(response_data.encode())

# Función que ejecuta el servidor HTTP
def run_server(server_class=HTTPServer, handler_class=MyHTTPRequestHandler, port=7800):
    server_address = ("", port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting server on port {port}...")
    httpd.serve_forever()

if __name__ == "__main__":
    # Inicia el servidor si se ejecuta este script como el programa principal
    run_server()
