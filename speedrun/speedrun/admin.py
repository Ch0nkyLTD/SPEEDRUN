from flask import Blueprint, request, jsonify
from speedrun.db import db
from speedrun.models import make_foo, Session, make_task, Task
from dataclasses import dataclass, asdict

admin = Blueprint("admin", __name__)


@admin.before_request
def before_request_interceptor():
    print(request.headers.get("auth") == "so super secret")
    print("im such a good middleware :)")


@admin.route("/session/task/create", methods=["POST"])
def handle_make_task():
    task_req = request.json
    session_id = task_req.get("session_id")
    s = db.session.query(Session).filter(Session.id == session_id).first()
    if s is None:
        return jsonify({"status": False, "msg": f"no session exsits for {session_id}"})
    t = make_task(**task_req)
    print(t)
    db.session.add(t)
    db.session.commit()
    return jsonify({"status": True, "task_id": t.id})


@admin.route("/session/task/list", methods=["GET"])
def handle_list_tasks():
    tasks = list(db.session.query(Task).all())
    return jsonify([asdict(item) for item in tasks])


@admin.route("/session/list", methods=["GET"])
def handle_list_sessions():
    sessions = list(db.session.query(Session).all())
    return jsonify([asdict(item) for item in sessions])
