from flask import Blueprint, request, jsonify
from speedrun.db import db
from speedrun.models import (
    make_foo,
    Session,
    make_task,
    Task,
    TASK_CREATED,
    TASK_RUNNING,
    TASK_STARTED,
)
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


@admin.route("/session/task/list2", methods=["GET"])
def get_active_tasks():
    data = request.get_json()

    if not data or "session_id" not in data:
        return jsonify({"error": "Session ID is required"}), 400

    session_id = data["session_id"]

    # Query tasks with the given session_id that are not completed or failed
    active_tasks = (
        Task.query.filter_by(session_id=session_id)
        .filter(Task.status.in_([TASK_CREATED, TASK_STARTED, TASK_RUNNING]))
        .all()
    )

    return jsonify([task.toJSON() for task in active_tasks]), 200


@admin.route("/session/task/list", methods=["GET"])
def handle_list_tasks():
    tasks = list(db.session.query(Task).all())
    return jsonify([asdict(item) for item in tasks])


@admin.route("/session/list", methods=["GET"])
def handle_list_sessions():
    sessions = list(db.session.query(Session).all())
    return jsonify([asdict(item) for item in sessions])
