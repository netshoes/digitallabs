using System;
using System.Net;
using System.Net.Http;
using System.Configuration;
using System.Threading.Tasks;

namespace BotTemplate.Core.QnAMaker
{
    [Serializable]
    public class QnAMakerService : IQnAMakerService
    {
        private string _qnaPostURL = ConfigurationManager.AppSettings["QnaPostURL"];
        private string _qnaHostName = ConfigurationManager.AppSettings["QnaHostName"];
        private string _qnaAuthorization = ConfigurationManager.AppSettings["QnaAuthorization"];

        public async Task<QnAMakerResult> TryQnAMakerAsync(string query)
        {
            try
            {
                using (var client = new HttpClient())
                {
                    var content = new StringContent("{\"question\":\"" + query + "\"}", System.Text.Encoding.UTF8, "application/json");
                    client.DefaultRequestHeaders.Add("Authorization", _qnaAuthorization);
                    var result = await client.PostAsync(_qnaHostName + _qnaPostURL, content);
                    if (result.StatusCode != HttpStatusCode.OK)
                    {
                        return new QnAMakerResult
                        {
                            Success = false,
                            Message = result.StatusCode.ToString(),
                            Content = null
                        };
                    }
                    else
                    {
                        var qnaresult = await result.Content.ReadAsAsync<QnAMakerContent>();
                        return new QnAMakerResult
                        {
                            Success = true,
                            Message = "",
                            Content = qnaresult
                        };
                    }
                }
            }
            catch (Exception e)
            {
                return new QnAMakerResult
                {
                    Success = false,
                    Message = e.Message,
                    Content = null
                };
            }
        }
    }
}