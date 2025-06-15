Import("env")
import os

def load_env():
    try:
        with open('.env') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    key, value = line.split('=', 1)
                    os.environ[key] = value.strip('"\'')
    except FileNotFoundError:
        print("Error: Missing .env file")
        env.Exit(1)

load_env()

REQUIRED = ["WIFI_SSID", "WIFI_PASS", "NOTIFY_EMAIL"]
for var in REQUIRED:
    if var not in os.environ:
        print(f"Error: Missing {var} in .env")
        env.Exit(1)

env.Append(
    BUILD_FLAGS=[
        f'-DWIFI_SSID=\\"{os.environ["WIFI_SSID"]}\\"',
        f'-DWIFI_PASS=\\"{os.environ["WIFI_PASS"]}\\"',
        f'-DNOTIFY_EMAIL=\\"{os.environ["NOTIFY_EMAIL"]}\\"'
    ]
)