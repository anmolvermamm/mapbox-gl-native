#include "animation_manager.h"


bool Compare(const std::unique_ptr<AnimationStyle>& left,
      const std::unique_ptr<AnimationStyle>& right)
{
   return left->milliseconds < right->milliseconds;
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
AnimationManager::AnimationManager(nlohmann::json animation_stylization) :
      m_animation_style(Compare)
{
   if(animation_stylization["animationStyles"] != nullptr)
   {
      for(const auto& style : animation_stylization["animationStyles"])
      {
         int id;
         (style["id"] != nullptr) ?
               id = style["id"].get<int>() :
               id = -1;

         float milliseconds;
         (style["milliseconds"] != nullptr) ?
               milliseconds = style["milliseconds"].get<float>() :
               milliseconds = 0;

         float opacity;
         (style["opacity"] != nullptr) ?
               opacity = style["opacity"].get<float>() :
               opacity = 1.0;

         float fill_opacity;
         (style["fillOpacity"] != nullptr) ?
               fill_opacity = style["fillOpacity"].get<float>() :
               fill_opacity = 1.0;

         //printf("%d, %f, %f, %f", id, milliseconds, opacity, fill_opacity);

         m_animation_style.insert(std::make_unique<AnimationStyle>(
               id, milliseconds, opacity,
               fill_opacity));
      }
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
void AnimationManager::UpdateTime(float milliseconds)
{
   if(m_current_milliseconds < 3000.0)
   {
      m_current_milliseconds += milliseconds;
   }
   else
   {
      m_current_milliseconds = milliseconds;
   }
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
float AnimationManager::GetCurrentOpacity()
{
   float opacity = 1.0;
   float max_milli = -1;
   for(const auto& animation_style : m_animation_style)
   {
      if(animation_style->milliseconds <= m_current_milliseconds)
      {
         if(max_milli < animation_style->milliseconds)
         {
            max_milli = animation_style->milliseconds;
            opacity = animation_style->opacity;
         }
      }
   }

   return opacity;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
float AnimationManager::GetCurrentFillOpacity()
{
   float opacity = 1.0;
   float max_milli = -1;
   for(const auto& animation_style : m_animation_style)
   {
      if(animation_style->milliseconds <= m_current_milliseconds)
      {
         if(max_milli < animation_style->milliseconds)
         {
            max_milli = animation_style->milliseconds;
            opacity = animation_style->fill_opacity;
         }
      }
   }

   return opacity;
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
AnimationManager::~AnimationManager()
{

}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
