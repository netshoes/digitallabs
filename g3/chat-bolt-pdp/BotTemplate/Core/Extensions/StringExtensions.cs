using System.Text;

namespace BotTemplate.Core.Extensions
{
    public static class StringExtensions
    {
        public static string ToUtf8(this string value)
        {
            var bytes = Encoding.Default.GetBytes(value);
            return Encoding.UTF8.GetString(bytes);
        }
    }
}