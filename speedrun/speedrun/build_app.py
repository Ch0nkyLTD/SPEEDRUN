from flask import Flask
from speedrun.db import db
from speedrun.demo import demo
from speedrun.lp import lp
from speedrun.admin import admin


def build_app():
    app = Flask(__name__)
    app.config.from_mapping(
        # HINT: use this for prod :)
        # SQLALCHEMY_DATABASE_URI="postgresql://speedrun:gofast@localhost:5432/src2"
        SQLALCHEMY_DATABASE_URI="sqlite:///c2.db"
    )
    app.register_blueprint(demo, url_prefix="/demo")
    app.register_blueprint(admin, url_prefix="/admin")
    app.register_blueprint(lp)
    db.init_app(app)
    return app


def init_db():
    db.drop_all()
    db.create_all()
