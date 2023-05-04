# System status of rdk program
uint8 status
# Measured TCP pose in base frame
float64[7] tcp_pose
# Measured flange pose in base frame
float64[7] flange_pose
# Measured raw force sensor data in flange frame
float64[6] raw_data
# Current node name
string node_name
# SPI config
string cpol
# SPI config
string cpha
# SPI config
string lsb
# SPI config
string selp
# Testman shakehand token
string token
# Testman task type ex. NORMAL BIAS DUMMY
string task_type
# Testman task name ex. Kostal-V216-MainPlan-NORMAL
string task_name
# Whether Testman query system status
string query_status
# Whether Testman terminate system
string terminate_status
