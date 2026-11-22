from flask import Flask, jsonify
from flask_cors import CORS

app = Flask(__name__)
CORS(app, origins = ["http://localhost:5173"])

#Temporário para teste, depois será substituído e/ou removido
buttonPlayPause = False

@app.route('/playpause')
def playpause():
    global buttonPlayPause
    if buttonPlayPause:
        buttonPlayPause = False
        print("Pausado")
        return jsonify({"status": "paused"})
    else:
        buttonPlayPause = True
        print("Continuando")
        return jsonify({"status": "playing"})

@app.route('/stop')
def stop():
    global buttonPlayPause
    buttonPlayPause = False
    print("Interrompendo")
    return jsonify({"status": "stopped"})

if __name__ == '__main__':
    app.run(port = 8000, debug = True)