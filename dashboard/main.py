from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import subprocess

app = Flask(__name__, static_url_path='', static_folder='frontend/build')
CORS(app)

@app.route("/", defaults={'path':''})
def serve(path):
    return send_from_directory(app.static_folder,'index.html')

@app.route("/", methods=["POST"])
def start_test():

    # Get the data from the POST request
    data = request.get_json()

    # Print the data to the console
    print("Received data: ")

    print(data)

    # Run the test with the data and get the results
    results = run_test(data)
    print(results)

    # Return the results in the expected format
    return jsonify(results)

def run_test(data):
    network_topology = data["networkTopology"]
    number_of_nodes = data["numberOfNodes"]
    congestion_control_algorithms = data["congestionControlAlgorithms"]
    rateOfCongLink = data["rateOfCongLink"]
    rateOfNodes = data["rateOfNodes"]
    print(network_topology, number_of_nodes, congestion_control_algorithms, rateOfCongLink, rateOfNodes)

    results = {"labels": [], "throughputs": []}

    for cc in congestion_control_algorithms:
        try:
            # Create separate stdout and stderr files for each test
            stdout_file = open(f"stdout_{cc}.txt", "w")
            stderr_file = open(f"stderr_{cc}.txt", "w")
            process = subprocess.Popen(
                ["./ns3", "run", "scratch/congestion_simulations/topology_1", "--", f"--nodes={number_of_nodes}", f"--tcp_cc={cc}", f"--linkDataRate={rateOfCongLink}", f"--rate={rateOfNodes}"],
                 cwd='/Users/swanav/Study/sem2/ee595-project/simulator/ns-allinone-3.38/ns-3.38', 
                 stdout=stdout_file, 
                 stderr=stderr_file,
            )
            out = process.wait()
            print(out)
            stdout_file.close()
            stderr_file.close()

            with open(f"stdout_{cc}.txt", "r") as f:
                throughput = f.read().split("Avg. Tx Throughput: ")[1].split(" kbps")[0]
                print(throughput)
                results["labels"].append(cc)
                results["throughputs"].append(throughput)
        except:
            print(f"Error running the test for {cc}")

    return results

if __name__ == "__main__":
    app.run()
