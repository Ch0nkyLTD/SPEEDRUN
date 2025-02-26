from flask import Blueprint, request, jsonify
from speedrun.db import db


demo = Blueprint("demo", __name__)


@demo.before_request
def before_request_interceptor():
    print("im such a good middleware :)")


@demo.route("/hello")
def hello():
    return jsonify({"hello": "there"})
