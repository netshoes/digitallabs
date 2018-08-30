(function() {
  var apiURL = 'https://mettainnovations.azure-api.net/arezzo';

  var video = document.getElementById('video');
  var canvas = document.getElementById('canvas');
  var context = canvas.getContext('2d');
  var pingResultTxt = document.getElementById('response');
  var jsonResultTxt = document.getElementById('json');

  var frameCounter = 0;
  var numberOfQueries = 0;

  console.log(adapter.browserDetails.browser);

  navigator.mediaDevices
    .getUserMedia({ video: true, audio: false })
    .then(function(stream) {
      video.srcObject = stream;
      video.play();
    })
    .catch(function(e) {
      alert(e.name);
    });

  if (ping()) {
    pingResultTxt.innerHTML = '<h2>API está online</h2>';
  }

  function snap() {
    canvas.width = video.clientWidth;
    canvas.height = video.clientHeight;
    context.drawImage(video, 0, 0);
  }

  function sendImage() {
    canvas.width = video.clientWidth;
    canvas.height = video.clientHeight;
    context.drawImage(video, 0, 0);
    var imgb64 = canvas.toDataURL('image/jpeg', 1.0);
    query(imgb64, jsonResultTxt);
  };

  function query(imgBase64, jsonResultTxt) {
    imgBase64 = imgBase64.substr(imgBase64.indexOf(',') + 1);
    $.ajax({
      type: 'POST',
      url: apiURL + '/query',
      beforeSend: function(xhrObj) {
        // Request headers
        xhrObj.setRequestHeader(
          'Ocp-Apim-Subscription-Key',
          ''
        );
        xhrObj.setRequestHeader('Content-Type', 'application/json');
      },
      processData: false,
      dataType: 'json',
      data: JSON.stringify({ image: imgBase64 })
    })
      .done(function(data) {
        localStorage.setItem('user_data', JSON.stringify(data));
        // jsonResultTxt.innerText = JSON.stringify(data);
        console.log(data);
        useStyleData();
        //alert(data);
      })
      .fail(function(data) {
        console.log(data);
        alert('Houve algum errro, atualize a página e tente novamente!');
      });
  }

  function ping() {
    var result = false;
    $.ajax({
      type: 'POST',
      url: apiURL + '/ping',
      beforeSend: function(xhrObj) {
        // Request headers
        xhrObj.setRequestHeader(
          'Ocp-Apim-Subscription-Key',
          ''
        );
      }
    })
      .done(function(data) {
        alert('API esta online');
        result = true;
      })
      .fail(function() {
        alert('error');
        result = false;
      })
      .always(function() {
        return result;
      });
  }
})();
