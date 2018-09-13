using System.Threading.Tasks;

namespace BotTemplate.Core.QnAMaker
{
    public interface IQnAMakerService
    {
        Task<QnAMakerResult> TryQnAMakerAsync(string query);
    }
}