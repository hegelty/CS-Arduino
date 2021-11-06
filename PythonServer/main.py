import asyncio
from fastapi import FastAPI
from datetime import datetime, timezone
from pytz import timezone

from pyfcm import FCMNotification

app = FastAPI()
global stat
stat = 0

FCM_APIKEY = "Your FCM API Key"
push_service = FCMNotification(FCM_APIKEY)


async def sendMessage(body, title):
    # 메시지 (data 타입)
    data_message = {
        "body": body,
        "title": title
    }

    # topic을 이용해 다수의 구독자에게 푸시알림을 전송함
    result = push_service.notify_topic_subscribers(topic_name="snack", data_message=data_message)

    # 전송 결과 출력
    print(result)

def write_log(content):
    LogFile = open("LogFile","a")
    LogFile.write(f"{datetime.now(timezone('Asia/Seoul')).strftime('%F %X')} {content}\n")
    LogFile.close()

@app.get("/log")
def show_log():
    try:
        LogFile = open("LogFile","r")
        logStr = LogFile.read()
        LogFile.close()
    except Exception as e:
        return "errer"
    return logStr

@app.post("/clear")
def clear_log():
    LogFile = open("LogFile","w")
    LogFile.write("")
    LogFile.close()
    write_log("Log Cleared")
    return "Log Cleared"

@app.get("/show")
def show_stat():
    global stat
    print(stat)
    return stat

@app.get("/open")
def open_box():
    global stat
    write_log(f"Open(mode : {stat})")
    if stat!=0: asyncio.run(sendMessage("간식 도둑이 나타났습니다!", "누군가가 지금 당신의 간식 상자를 열었습니다."))
    return stat

@app.get("/close")
def close_box():
    global stat
    write_log(f"Close(mode : {stat})")
    return stat

@app.get("/set/{statnum}")
def set(statnum: int):
    global stat
    stat = statnum;
    write_log(f"Set(mode : {stat})")
    print(stat)
    return stat

@app.get("/token/{token_str}")
def add_token(token_str: str):
    with open("TOKEN", "a") as f:
        f.write(token_str+"\n")
    return "success"