import cv2
import numpy as np
import serial
import time

# === Serial Setup ===
arduino = serial.Serial('COM3', 9600)
time.sleep(2)

cap = cv2.VideoCapture(0)
frame_height = int(cap.get(4))

last_angle = 90
angle_threshold = 3

while True:
    ret, frame = cap.read()
    if not ret:
        continue

    # === Preprocess ===
    blurred = cv2.GaussianBlur(frame, (9, 9), 0)
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

    # === Yellow HSV Mask (Tuned) ===
    lower_yellow = np.array([22, 120, 120])
    upper_yellow = np.array([32, 255, 255])
    mask = cv2.inRange(hsv, lower_yellow, upper_yellow)

    # === Clean the mask ===
    mask = cv2.erode(mask, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=4)
    mask = cv2.GaussianBlur(mask, (7, 7), 0)

    contours, _ = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    if contours:
        # Largest yellow blob
        c = max(contours, key=cv2.contourArea)
        area = cv2.contourArea(c)

        if area > 800:  # Ignore small objects
            x, y, w, h = cv2.boundingRect(c)
            center_y = y + h // 2

            # === Map center to vertical angle ===
            angle = int((center_y / frame_height) * 180)
            angle = np.clip(angle, 0, 180)

            # === Only update if significant change ===
            if abs(angle - last_angle) > angle_threshold:
                arduino.write(f"{angle}\n".encode())
                last_angle = angle
                time.sleep(0.1)

            # === Visual Feedback ===
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.circle(frame, (x + w // 2, center_y), 6, (0, 0, 255), -1)
            cv2.putText(frame, f"Aim: {angle}°", (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)

    cv2.imshow("Accurate Yellow Detection", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
arduino.close()
