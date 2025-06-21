# backend/genai.py

import os
import cohere
from dotenv import load_dotenv

load_dotenv()

COHERE_API_KEY = os.getenv("COHERE_API_KEY")
co = cohere.Client(COHERE_API_KEY) if COHERE_API_KEY else None

def generate_summary(diff_text):
    """Basic summary generation"""
    try:
        if not co:
            return "AI analysis unavailable - Cohere API key not configured"
            
        response = co.generate(
            model='command',
            prompt=f"Explain in detail the following LLVM IR diff and describe the optimizations or changes applied:\n\n{diff_text}",
            max_tokens=500,
            temperature=0.5
        )
        return response.generations[0].text.strip()
    except Exception as e:
        return f"Error from Cohere API: {str(e)}"

def generate_enhanced_summary(diff_text, before_lines, after_lines, passes, metrics):
    """Enhanced summary generation with additional context"""
    try:
        if not co:
            return "AI analysis unavailable - Cohere API key not configured"
            
        # Create enhanced prompt with metrics
        enhanced_prompt = f"""
        Analyze this LLVM IR optimization in detail:
        
        OPTIMIZATION PASS: {passes}
        
        METRICS:
        - Instructions before: {len(before_lines)} lines
        - Instructions after: {len(after_lines)} lines
        - Changes: {metrics.get('instructions_eliminated', 'unknown')} instructions
        
        DIFF:
        {diff_text[:1000]}...
        
        Please provide:
        1. What optimization was applied
        2. Key changes made
        3. Performance impact
        4. Technical explanation
        """
        
        response = co.generate(
            model='command',
            prompt=enhanced_prompt,
            max_tokens=800,
            temperature=0.3
        )
        return response.generations[0].text.strip()
        
    except Exception as e:
        # Fallback to basic analysis
        return f"""
        ## LLVM IR Optimization Analysis
        
        **Pass Applied:** {passes}
        
        **Summary:** Applied {passes} optimization pass to the LLVM IR code.
        
        **Changes:** 
        - Lines before: {len(before_lines)}
        - Lines after: {len(after_lines)}
        - Net change: {len(after_lines) - len(before_lines)} lines
        
        **Note:** Detailed AI analysis unavailable - {str(e)}
        """
