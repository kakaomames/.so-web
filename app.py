from flask import Flask, jsonify
import subprocess
import os

app = Flask(__name__)
ATTACK_PATH = "/data/data/com.termux/files/home/attack"

def mission_log(action, message):
    print(f"[{action}] 📋 {message} ✨")

@app.route('/api/so/<func_name>/<arg>')
def native_bridge(func_name, arg):
    # 隊長のアドバイス：現在の環境変数（PATH等）をすべて引き継ぎつつ
    # LD_LIBRARY_PATH を上書きする
    env = os.environ.copy()
    env["LD_LIBRARY_PATH"] = "/data/data/com.termux/files/usr/lib"
    
    # 隊員提案の shell=True プロトコル
    cmd = f"{ATTACK_PATH} {func_name} {arg}"
    mission_log("EXEC", cmd)

    try:
        # env=env を指定して、システム環境を完全に再現
        proc = subprocess.run(
            cmd, 
            shell=True, 
            capture_output=True, 
            text=True, 
            env=env
        )

        if proc.returncode == 0:
            mission_log("SUCCESS", "生還！")
            return jsonify({
                "status": "success",
                "output": proc.stdout.strip()
            })
        else:
            # 失敗しても、何が起きたか（stdoutも含めて）全部返す
            mission_log("FAIL", f"ReturnCode: {proc.returncode}")
            return jsonify({
                "status": "error",
                "code": proc.returncode,
                "stdout": proc.stdout.strip(),
                "stderr": proc.stderr.strip()
            }), 500
    except Exception as e:
        return jsonify({"status": "fatal", "message": str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
