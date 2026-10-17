
from pathlib import Path
import json
import subprocess
import csv

from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

PROJECT_ROOT = Path(__file__).resolve().parent.parent
ENGINE_PATH = PROJECT_ROOT / "build" / "SmartReliefEngine"


@app.get("/")
def home():
    return render_template("index.html")


@app.get("/about")
def about():
    return render_template("about.html")



@app.post("/api/optimize")
def optimize():
    data = request.get_json(silent=True)

    if not isinstance(data, dict):
        return jsonify({"success": False, "error": "Expected a JSON object."}), 400

    try:
        capacity = data.get("capacity")
        items = data.get("items")
        algorithm = data.get("algorithm", "dp")

        if isinstance(capacity, bool) or not isinstance(capacity, int) or capacity < 0:
            return jsonify({
                "success": False,
                "error": "Capacity must be a non-negative integer."
            }), 400

        if not isinstance(items, list):
            return jsonify({
                "success": False,
                "error": "Items must be a list."
            }), 400

        allowed_algorithms = {
            "recursive",
            "dp",
            "greedy",
            "branch_bound"
        }

        if algorithm not in allowed_algorithms:
            return jsonify({
                "success": False,
                "error": "Unknown algorithm."
            }), 400

        if len(items) > 25:
            return jsonify({
                "success": False,
                "error": "Maximum 25 items per request for now."
            }), 400

        normalized_items = []

        for item in items:
            if not isinstance(item, dict):
                raise ValueError("Each item must be a JSON object.")

            name = item.get("name")
            weight = item.get("weight")
            value = item.get("value")

            if not isinstance(name, str) or not name.strip():
                raise ValueError("Each item needs a non-empty name.")

            if isinstance(weight, bool) or not isinstance(weight, int) or weight <= 0:
                raise ValueError("Item weights must be positive integers.")

            if isinstance(value, bool) or not isinstance(value, int) or value < 0:
                raise ValueError("Item values must be non-negative integers.")

            normalized_items.append({
                "name": name.strip(),
                "weight": weight,
                "value": value
            })

        payload = {
            "algorithm": algorithm,
            "capacity": capacity,
            "items": normalized_items
        }

        if not ENGINE_PATH.is_file():
            return jsonify({
                "success": False,
                "error": "C++ engine not found. Build SmartReliefEngine first."
            }), 500

        result = subprocess.run(
            [str(ENGINE_PATH)],
            input=json.dumps(payload),
            capture_output=True,
            text=True,
            timeout=10,
            check=False
        )

        if result.returncode != 0:
            try:
                engine_error = json.loads(result.stdout)
                message = engine_error.get("error", "Engine execution failed.")
            except json.JSONDecodeError:
                message = "Engine execution failed."

            return jsonify({
                "success": False,
                "error": message
            }), 400

        try:
            response = json.loads(result.stdout)
        except json.JSONDecodeError:
            return jsonify({
                "success": False,
                "error": "C++ engine returned invalid JSON."
            }), 500

        return jsonify(response)

    except ValueError as error:
        return jsonify({
            "success": False,
            "error": str(error)
        }), 400

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "The algorithm took too long. Try fewer items."
        }), 408

    except Exception:
        app.logger.exception("Optimization request failed.")
        return jsonify({
            "success": False,
            "error": "An unexpected server error occurred."
        }), 500


if __name__ == "__main__":
    app.run(debug=True)

@app.get("/api/benchmarks")
def get_benchmarks():
    csv_path = PROJECT_ROOT / "benchmarks" / "results" / "benchmark_results.csv"

    if not csv_path.is_file():
        return jsonify({
            "success": False,
            "error": "Benchmark results not found. Run SmartReliefBenchmark first."
        }), 404

    try:
        with csv_path.open("r", newline="", encoding="utf-8") as file:
            reader = csv.DictReader(file)
            rows = list(reader)

        return jsonify({
            "success": True,
            "count": len(rows),
            "results": rows
        })

    except (OSError, csv.Error):
        app.logger.exception("Could not read benchmark results.")
        return jsonify({
            "success": False,
            "error": "Could not read benchmark results."
        }), 500