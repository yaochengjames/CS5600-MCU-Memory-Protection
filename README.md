# CS5600 MCU Memory Protection Project

## Project Structure
```
├── baseline_and_static/    # Baseline and Static MPU implementations
├── dynamic_mpu/            # Dynamic MPU implementation (modified FreeRTOS)
└── results/                # Experiment results
```

## Build and Run

### Baseline Configuration
```bash
cd baseline_and_static
make CONFIG=baseline
make CONFIG=baseline run
```
### Static MPU Configuration
```bash
cd baseline_and_static
make CONFIG=static
make CONFIG=static run
```

### Dynamic MPU Configuration
```bash
cd dynamic_mpu
make run
```



