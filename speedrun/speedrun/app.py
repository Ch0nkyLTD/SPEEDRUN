from speedrun.build_app import build_app

app = build_app()


@app.route("/hello")
def hello():
    return "hello there"
