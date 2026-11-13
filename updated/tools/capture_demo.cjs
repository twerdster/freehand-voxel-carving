// Optional capture tool: node capture_demo.cjs OUTPUT_DIRECTORY (Playwright + Chrome).
const {chromium}=require(process.env.PLAYWRIGHT_MODULE || 'playwright');
const fs=require('fs');
(async()=>{
 const output=require('path').resolve(process.argv[2] || 'output/freehand');
 fs.mkdirSync(output+'/frames',{recursive:true});
 const browser=await chromium.launch({channel:'chrome',headless:true,args:['--enable-unsafe-swiftshader']});
 const page=await browser.newPage({viewport:{width:940,height:760},deviceScaleFactor:1});
 const errors=[];page.on('pageerror',error=>errors.push(error.message));
 await page.goto('file://'+output+'/viewer.html');await page.waitForFunction(()=>window.ready===true);await page.evaluate(()=>window.pause());
 await page.evaluate(()=>window.renderFrame(270));await page.screenshot({path:output+'/reconstruction.png',fullPage:true});
 for(let i=0;i<48;i++){await page.evaluate(angle=>window.renderFrame(angle),(270+i*360/48)%360);await page.locator('#stage').screenshot({path:output+'/frames/frame-'+String(i).padStart(3,'0')+'.png'});}
 fs.writeFileSync(output+'/browser-qa.json',JSON.stringify({page_errors:errors,frames:48,viewport:[940,760],canvas:await page.locator('canvas').evaluate(c=>[c.width,c.height]),webgl:await page.locator('canvas').evaluate(c=>c.getContext('webgl').getParameter(c.getContext('webgl').VERSION))},null,2));
 await browser.close();if(errors.length)process.exit(1);
})();
