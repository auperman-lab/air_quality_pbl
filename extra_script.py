Import("env")
import os

def load_env():
    try:
        with open('.env') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                key, value = line.split('=', 1)
                os.environ[key] = value.strip('"\'')
    except FileNotFoundError:
        print("Warning: .env file not found")

load_env()

# Ensure required vars are present
REQUIRED = ["BLYNK_AUTH", "WIFI_SSID", "WIFI_PASS", "NOTIFY_EMAIL"]
for v in REQUIRED:
    if v not in os.environ:
        print(f"Error: Missing {v}")
        env.Exit(1)

# Inject into build flags
env.Append(
    BUILD_FLAGS=[
        f'-DBLYNK_AUTH=\"{os.environ["BLYNK_AUTH"]}\"',
        f'-DWIFI_SSID=\"{os.environ["WIFI_SSID"]}\"',
        f'-DWIFI_PASS=\"{os.environ["WIFI_PASS"]}\"',
        f'-DNOTIFY_EMAIL=\"{os.environ["NOTIFY_EMAIL"]}\"',
    ]
)