namespace BotTemplate.Core.QnAMaker
{
    public class QnAMakerResult
    {
        public QnAMakerContent Content { get; set; }
        public bool Success { get; set; }
        public string Message { get; set; }
    }
    public class QnAMakerContent
    {
        public QnAMakerAnswer[] answers { get; set; }
    }
    public class QnAMakerAnswer
    {
        public string[] questions { get; set; }
        public string answer { get; set; }
        public float score { get; set; }
        public int id { get; set; }
        public string source { get; set; }
        public object[] metadata { get; set; }
    }
}