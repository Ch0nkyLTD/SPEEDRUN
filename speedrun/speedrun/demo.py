from flask import Blueprint, request, jsonify
from speedrun.db import db
from speedrun.models import make_foo, Foo
from dataclasses import dataclass, asdict

demo = Blueprint("demo", __name__)


@demo.before_request
def before_request_interceptor():
    print("im such a good middleware :)")


@demo.route("/hello")
def hello():
    return jsonify({"hello": "there"})


@demo.route("/foo/list", methods=["GET"])
def list_foo():
    foos = list(Foo.query.all())
    print(foos, [asdict(i) for i in foos])
    return jsonify([asdict(item) for item in foos])


@demo.route("/foo/create", methods=["POST"])
def create_foo():
    json_data = request.json
    print("make_foo", json_data)
    try:
        t = make_foo(**json_data)
        db.session.add(t)
        db.session.commit()
        return jsonify({"status": True, "msg": f"created {t.id}"})

    except Exception as e:
        print(Exception, e, "Failed to create task :(")
        # todo: better error handling/messaging
        return jsonify({"status": False, "msg": "Failed to create task"})
