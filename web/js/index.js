var vueApp = new Vue({
    el: '#vue-data',
    data: {
      message: 'Hello Vue !',
      accel_x: '0.00',
      accel_y: '0.00',
      accel_z: '0.00',
      gyr_x: '0.00',
      gyr_y: '0.00',
      gyr_z: '0.00',

      mode:'default',
      saving:'',
      displayGains:'',
      temp: '25.0',
      pressure:'1000',

      ki:'i',
      kd:'d',
      kp:'p',

      gps_fix:'',
      gps_latitude:'',
      gps_longitude:'',
      host: localStorage.websocket_url,
      frequency: '0'

    }
  })