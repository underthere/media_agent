import requests

# add source
resp = requests.post(
    "http://localhost:18080/add_source",
    json = {
        "id": "kkyy",
        "media_description": {
            "protocol": "rtmp",
            "uri": "rtmp://dev.smt.dyinnovations.com/live/test",
            "custom_data": "",
            "audio_description": {},
            "video_description": {
                "width": 1920,
                "height": 1080,
                "fps": 25,
                "pixel_format": "",
                "codec_format": "h264",
                "bitrate": 2000000,
                "profile": "baseline",
                "level": 31,
            }
        }
    }
)

print(resp.status_code, resp.text)