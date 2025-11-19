import random, time

def simulate_point():
    return {
        "depths_cm": [3, 5, 7],
        "soil_raw": [random.randint(300,700) for _ in range(3)],
        "temp_c": round(24 + random.uniform(-2,2), 1),
        "humidity": round(60 + random.uniform(-5,5), 1)
    }

if __name__ == "__main__":
    print("Simulating 6 sampling points...")
    for i in range(6):
        p = simulate_point()
        print(
            f"Point {i+1}: depths={p['depths_cm']} | raw={p['soil_raw']} "
            f"| T={p['temp_c']}C | H={p['humidity']}%"
        )
        time.sleep(0.3)
